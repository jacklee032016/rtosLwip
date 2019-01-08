

#include "compact.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "sysclk.h"
#include "ioport.h"

#include "lwipExt.h"

#include "eos.h"


#include "sha256_software.h"


struct SHA256_ds_msg 
{
	unsigned char rom_id[8];		//7 byte id(start from 0x17) + 1 byte CRC
	unsigned char secret[32];		//secret write to chip
	unsigned char challenge[32];	//random data write to chip
	unsigned char mac[32];			//mac data read from chip
	unsigned char page_0_data[32];	//page 0 data write to/read from chip
	unsigned char page_1_data[32];	//page 1 data write to/read from chip
	unsigned char man_id[2];		//manufacturer ID
	unsigned char page_num;			//page number, which page is used for MAC 
	unsigned char anon;				//Anonymous Indicator(ANON). These bits specify whether the device’s ROM ID is used for the MAC computation
									//1--anonymous mode, 0--NOT anonymous mode.
	unsigned char lock_flag;		//1-lock secret, secret is fixed for ever	
	unsigned char lock_magic;		//when (logic_magic == 0x5A)&&(lock_flag == 1), lock the secret
};

//compute rom_id crc8
int calc_rom_id_crc(int data_byte, int crc)
{
	int bit_mask = 0, carry_check = 0, temp_data = 0;
	temp_data = data_byte;
	for ( bit_mask = 0; bit_mask <= 7; bit_mask ++)
	{
		data_byte = data_byte ^ crc;
		crc = crc / 2;
		temp_data = temp_data / 2;
		carry_check = data_byte & 0x01;
		if (carry_check)
		{
			crc = crc ^ 0x8C;
		}
		data_byte = temp_data;
	}
	return ( crc );
}



//int main(int argc,char **argv)
int sha256_app_check(void)
{
	int fd;
	int ret, i;
	int testcrc, data_byte;
	unsigned char challenge[32];
	unsigned char secret[32];
	unsigned char chip_mac[32];
	unsigned char page_data[32];
	unsigned char rom_id[8];
	unsigned char man_id[2];
	unsigned char personality_data[4];
	unsigned char page_num;
	unsigned char anon;
	unsigned char lock_magic;
	struct SHA256_ds_msg	chip_ds_msg;
	
	//init chip_ds_msg
	for (i = 0; i < 8; i++)
	{
		chip_ds_msg.rom_id[i] = 0;
		chip_ds_msg.secret[i] = 0x5a;
		chip_ds_msg.challenge[i] = 0xa9;
	}		
	for (i = 0; i < 20; i++)
		chip_ds_msg.mac[i] = 0x0;
	for (i = 0; i < 32; i++)
		chip_ds_msg.page_0_data[i] = 0x0;	
	for (i = 0; i < 32; i++)
		chip_ds_msg.page_1_data[i] = 0x0;		
	chip_ds_msg.lock_flag = 0;
	chip_ds_msg.lock_magic = 0;
		
	anon = 0;
	testcrc = 0x00;	
	fd =open("/dev/muxds",O_RDWR);
	if(fd < 0 )
	{
		printf("can not open /dev/muxds!\n");
		return -1;
	}	
	printf("Step 1: Read DS28E15 ROM ID. Check CRC.\n");
	ret = ioctl(fd, SHA256_READ_ROM_ID, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL READ_ROM read ds error!\n");
		return -2;
	}
	
	memcpy(rom_id, &chip_ds_msg.rom_id[0], 8);	
	
	testcrc = 0x00;		
	for(i = 0; i < 7; i++)
	{
		data_byte = rom_id[i];			
		testcrc = calc_rom_id_crc(data_byte, testcrc);
	}	
	if((testcrc == rom_id[7]) && (rom_id[0] == 0x17))		
		printf("read ROM ID OK!\n");
	else
	{
		printf("read ROM ID FAIL! Exit!\n");	
		return -3;
	}	
	
	printf("Step 2: Read page data, init challenge, read mac from chip and verfify.\n");
	ret = ioctl(fd, SHA256_READ_CHIP_MEM, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL READ_CHIP_MEM FAIL!\n");
		return -4;
	}		
	srand((unsigned)time(NULL));	//init random seed	
	for (i = 0; i < 32; i++)
	{	
		chip_ds_msg.challenge[i] = rand()%256;		
	}
	
	//set page_num	
	chip_ds_msg.page_num = 0;
	//set no anonymous mode
	chip_ds_msg.anon = 0;		
	
	//Read MAC from chip, this step does not need to send secret to chip. ONLY 32 byte challenge data is used.
	ret = ioctl(fd, SHA256_COMPUTE_READ_MAC, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL COMPUTE_READ_MAC FAIL!\n");
		return -5;
	}
	
	anon = chip_ds_msg.anon;
	
	page_num = chip_ds_msg.page_num;
	if(page_num == 0)
		memcpy(page_data, chip_ds_msg.page_0_data, 32);
	else
		memcpy(page_data, chip_ds_msg.page_1_data, 32);	
	
	memcpy(challenge, chip_ds_msg.challenge, 32);	
	memcpy(man_id, chip_ds_msg.man_id, 2);
	memcpy(chip_mac, chip_ds_msg.mac, 32);
	
	//setcret key
	for (i = 0; i < 32; i++)		
		secret[i] = i;		
	
	secret[0] = rom_id[0];
	secret[1] = rom_id[1];
	secret[2] = rom_id[2];
	secret[3] = rom_id[3];
	secret[4] = rom_id[4];
	secret[5] = rom_id[5];
	secret[6] = rom_id[6];
	secret[7] = rom_id[7];	
	
	printf("page_num : %d, anon: %d\n", page_num, anon);
	printf("Rom ID: ");
	for (i = 0; i < 8; i++)		
		printf(" 0x%02X ", rom_id[i]);
	printf("\n");
	printf("Challenge : \n");
	for (i = 0; i < 32; i++)		
		printf(" 0x%02X ", challenge[i]);
	printf("\n");
	printf("Chip MAC: \n");
	for (i = 0; i < 32; i++)		
		printf(" 0x%02X ", chip_mac[i]);
	printf("\n");
	if(!Compute_Software_Mac(page_num, page_data, challenge, rom_id, secret, man_id, anon, chip_mac))
	{
		printf("MAC compare ERROR!\n");
		return -6;
	}
	else
		printf("MAC compare OK!\n");	
		
	close(fd);
	return 0;		
}


int eosScKey(int argc,char **argv)
{
	int fd;
	int cmd, ret, i, errnum,totalnum;
	unsigned int value;
	unsigned char dev_info[8];
	int testcrc, data_byte;
	struct ds_msg	chip_ds_msg;
	unsigned char secret[32];
	
	//init chip_ds_msg
	memset(&chip_ds_msg.rom_id[0], 0, sizeof(chip_ds_msg));
	/*
	for (i = 0; i < 8; i++)
	{
		chip_ds_msg.rom_id[i] = 0;
		chip_ds_msg.challenge[i] = 0xa9;
	}	
	for (i = 0; i < 32; i++)
		chip_ds_msg.secret[i] = 0x5a;
	chip_ds_msg.flag = 0;
	for (i = 0; i < 20; i++)
		chip_ds_msg.mac[i] = 0x0;
	for (i = 0; i < 32; i++)
		chip_ds_msg.page_data[i] = 0x0;	
	chip_ds_msg.flag = 0;
	*/
	testcrc = 0x00;
	fd =open("/dev/muxds",O_RDWR);
	if(fd < 0)
	{
		printf("can not open /dev/muxds!\n");
		return -1;
	}
	
	totalnum = 0; errnum = 0;
	//while(1)
	{		
		totalnum++;
		memset(dev_info, 0, sizeof(dev_info));
		ret = ioctl(fd, SHA256_READ_ROM_ID, &chip_ds_msg);
		if(ret != 0)
		{
			printf("DS IOCTL READ_ROM read ds error!\n");
		}		
		for(i = 0; i < 8; i++)
		{
			dev_info[i] = chip_ds_msg.rom_id[i];
			printf(" 0x%02x ", dev_info[i]);		
		}					
		testcrc = 0x00;		
		for(i = 0; i < 7; i++)
		{
			data_byte = dev_info[i];			
			testcrc = calc_crc(data_byte, testcrc);
		}
		printf("CRC: 0x%x ", testcrc);
	
		if( (testcrc == dev_info[7]) && ( dev_info[0] == 0x17) )	//check device family code 0x17 for DS28E15
		{	
			printf("OK!");
		}
		else
		{
			printf("ERROR!");
			errnum++;
		}
		printf("ratio:%d/%d\n", errnum, totalnum);
		
		secret[0] = (dev_info[0] + 0x2A + 0x2) & 0xFF;
		secret[1] = (dev_info[1] + 0x4D + 0x2) & 0xFF;
		secret[2] = (dev_info[2] + 0x41 + 0x2) & 0xFF;
		secret[3] = (dev_info[3] + 0x52 + 0x2) & 0xFF;
	
		secret[4] = (dev_info[4] + 0x42 + 0x2) & 0xFF;
		secret[5] = (dev_info[5] + 0x4F + 0x2) & 0xFF;
		secret[6] = (dev_info[6] + 0x48 + 0x2) & 0xFF;
		secret[7] = (dev_info[7] + 0x4A + 0x2) & 0xFF;
	
		secret[8]  = (dev_info[0] + 0x4F + 0x4) & 0xFF;
		secret[9]  = (dev_info[1] + 0x43 + 0x4) & 0xFF;
		secret[10] = (dev_info[2] + 0x41 + 0x4) & 0xFF;
		secret[11] = (dev_info[3] + 0x55 + 0x4) & 0xFF;
	
		secret[12] = (dev_info[4] + 0x42 + 0x4) & 0xFF;
		secret[13] = (dev_info[5] + 0x50 + 0x4) & 0xFF;
		secret[14] = (dev_info[6] + 0x45 + 0x4) & 0xFF;
		secret[15] = (dev_info[7] + 0x54 + 0x4) & 0xFF;
	
		secret[16] = (dev_info[0] + 0x4A + 0x6) & 0xFF;	
		secret[17] = (dev_info[1] + 0x49 + 0x6) & 0xFF;
		secret[18] = (dev_info[2] + 0x41 + 0x6) & 0xFF;
		secret[19] = (dev_info[3] + 0x52 + 0x6) & 0xFF;
	
		secret[20] = (dev_info[4] + 0x4F + 0x6) & 0xFF;
		secret[21] = (dev_info[5] + 0x44 + 0x6) & 0xFF;
		secret[22] = (dev_info[6] + 0x46 + 0x6) & 0xFF;
		secret[23] = (dev_info[7] + 0x41 + 0x6) & 0xFF;
	
		secret[24] = (dev_info[0] + 0x4E + 0x8) & 0xFF;	
		secret[25] = (dev_info[1] + 0x53 + 0x8) & 0xFF;
		secret[26] = (dev_info[2] + 0x41 + 0x8) & 0xFF;
		secret[27] = (dev_info[3] + 0x4E + 0x8) & 0xFF;
	
		secret[28] = (dev_info[4] + 0x50 + 0x8) & 0xFF;
		secret[29] = (dev_info[5] + 0x41 + 0x8) & 0xFF;
		secret[30] = (dev_info[6] + 0x54 + 0x8) & 0xFF;
		secret[31] = (dev_info[7] + 0x2A + 0x8) & 0xFF;
	
		printf("\nsecret key is : \n");
		for(i = 0; i < 32; i++)
			printf("%02x", secret[i]);
		
		printf("\n");
		usleep(100000);
	}
	
	close(fd);
	return 0;		
}


//int main(int argc,char **argv)
int chip_mac_test(int seed)
{
	int fd;
	int ret, i;
	int testcrc, data_byte;
	unsigned char challenge[32];
	unsigned char secret[32];
	unsigned char chip_mac[32];
	unsigned char page_data[32];
	unsigned char rom_id[8];
	unsigned char man_id[2];
	unsigned char personality_data[4];
	unsigned char page_num;
	unsigned char anon;
	struct SHA256_ds_msg	chip_ds_msg;
	unsigned char page_1_data[32];
	
	//init chip_ds_msg
	memset(&chip_ds_msg.rom_id[0], 0, sizeof(chip_ds_msg));
	/*
	for (i = 0; i < 8; i++)
	{
		chip_ds_msg.rom_id[i] = 0;
		chip_ds_msg.secret[i] = 0x5a;		
	}		
	for (i = 0; i < 32; i++)
		chip_ds_msg.challenge[i] = 0x0;	
	for (i = 0; i < 32; i++)
		chip_ds_msg.mac[i] = 0x0;
	for (i = 0; i < 32; i++)
		chip_ds_msg.page_0_data[i] = 0x0;	
	for (i = 0; i < 32; i++)
		chip_ds_msg.page_1_data[i] = 0x0;	
	chip_ds_msg.lock_flag = 0;		
	chip_ds_msg.lock_magic = 0;	
	*/
	anon = 0;
	testcrc = 0x00;	
	fd =open("/dev/muxds",O_RDWR);
	if(fd < 0 )
	{
		printf("can not open /dev/muxds!\n");
		return -1;
	}
	printf("Step 1: DS28E01 CHIP timing init.\n");
	ret = ioctl(fd, SHA256_CHIP_TIMING_INIT, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL CHIP_TIMING_INIT error!\n");
		return -2;
	}
	printf("Step 2: Read DS28E01 ROM ID. Check CRC.\n");
	
	ret = ioctl(fd, SHA256_READ_ROM_ID, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL READ_ROM read ds error!\n");
		return -3;
	}
	
	memcpy(rom_id, &chip_ds_msg.rom_id[0], 8);	
	
	testcrc = 0x00;		
	for(i = 0; i < 7; i++)
	{
		data_byte = rom_id[i];			
		testcrc = calc_rom_id_crc(data_byte, testcrc);
	}	
	if((testcrc == rom_id[7]) && (rom_id[0] == 0x17))		
		printf("read ROM ID OK!\n");
	else
	{
		printf("read ROM ID FAIL! Exit!\n");	
		return -4;
	}	
	for (i = 0; i < 8; i++)		
		printf(" 0x%x ", rom_id[i]);
	printf("\n");
	
	printf("Step 3: Init memory page data & secret to DS28E15.\n");
	
	//init setcret key
	for (i = 0; i < 32; i++)		
		chip_ds_msg.secret[i] = i;		
	
	chip_ds_msg.secret[0] = rom_id[0];
	chip_ds_msg.secret[1] = rom_id[1];
	chip_ds_msg.secret[2] = rom_id[2];
	chip_ds_msg.secret[3] = rom_id[3];
	chip_ds_msg.secret[4] = rom_id[4];
	chip_ds_msg.secret[5] = rom_id[5];
	chip_ds_msg.secret[6] = rom_id[6];
	chip_ds_msg.secret[7] = rom_id[7];	
	/*
	rom_id[0] = 0x17;
	rom_id[1] = 0x5C;
	rom_id[2] = 0x44;
	rom_id[3] = 0x59;
	rom_id[4] = 0x0;
	rom_id[5] = 0x0;
	rom_id[6] = 0x0;
	rom_id[7] = 0xa6;
	chip_ds_msg.secret[0] = 0x17;
	chip_ds_msg.secret[1] = 0x5C;
	chip_ds_msg.secret[2] = 0x44;
	chip_ds_msg.secret[3] = 0x59;
	chip_ds_msg.secret[4] = 0x0;
	chip_ds_msg.secret[5] = 0x0;
	chip_ds_msg.secret[6] = 0x0;
	chip_ds_msg.secret[7] = 0xa6;
	*/
	//init memory page 0 data
	for (i = 0; i < 32; i++)		
		chip_ds_msg.page_0_data[i] = i + 2 + seed;		
	
	//init memory page 1 data
	for (i = 0; i < 32; i++)		
		chip_ds_msg.page_1_data[i] = i + 30 + seed;			
	
	chip_ds_msg.page_num = 0;
	chip_ds_msg.anon = 0;
	chip_ds_msg.lock_flag = 0;	//Do not lock secret
	chip_ds_msg.lock_magic = 0;
	
	ret = ioctl(fd, SHA256_INIT_CHIP_PRODUCTION_KEY, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL INIT_CHIP_PRODUCTION FAIL!\n");
		return -5;
	}	
	
	printf("Step 4: Write random ethernet MAC address into page 1 of DS28E15.\n");
	//PAGE 1 structure	
	//[0-5] 6 byte MAC address, [10]-- model_id, [20]--product_name_id, [31]--CRC8 of [0]~[30])
	//init memory page 1 data to 0.
	for (i = 0; i < 32; i++)		
		chip_ds_msg.page_1_data[i] = 0;			
	
	//Random MAC
	chip_ds_msg.page_1_data[0] = 0x00;
	chip_ds_msg.page_1_data[1] = rand()%256;
	chip_ds_msg.page_1_data[2] = rand()%256;
	chip_ds_msg.page_1_data[3] = rand()%256;
	chip_ds_msg.page_1_data[4] = rand()%256;
	chip_ds_msg.page_1_data[5] = rand()%256;	
	
	//model_id
	chip_ds_msg.page_1_data[10]  = 0x01;
	
	//product_name_id
	chip_ds_msg.page_1_data[20]  = 0x01;
	
	//get CRC8 of page 1 data. From [0] to [30]
	testcrc = 0x00;		
	for(i = 0; i < 31; i++)
	{
		data_byte = chip_ds_msg.page_1_data[i];			
		testcrc = calc_rom_id_crc(data_byte, testcrc);
	}	
	chip_ds_msg.page_1_data[31]  = testcrc;
	
	memcpy(page_1_data, &chip_ds_msg.page_1_data[0], 32);
	
	chip_ds_msg.page_num = 0;
	chip_ds_msg.anon = 0;
	chip_ds_msg.lock_flag = 0;	//Do not lock secret
	chip_ds_msg.lock_magic = 0;
		
	ret = ioctl(fd, SHA256_INIT_CHIP_PRODUCTION_MAC, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL INIT_CHIP_PRODUCTION_MAC FAIL!\n");
		return -6;
	}	
		
	printf("\nStep 5: Read page data, init challenge, read mac from chip and verfify.\n");
	ret = ioctl(fd, SHA256_READ_CHIP_MEM, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL READ_CHIP_MEM FAIL!\n");
		return -7;
	}		
	srand((unsigned)time(NULL));	//init random seed	
	for (i = 0; i < 32; i++)
	{	
		chip_ds_msg.challenge[i] = rand()%256;		
	}

	for (i = 0; i < 32; i++)
	{	
		chip_ds_msg.challenge[i] = rand()%256;		
	}
	
	printf("Read Page 1 data is :\n");
	for(i = 0; i < 32; i++)
	{
		printf(" 0x%02x ", chip_ds_msg.page_1_data[i]);
		if(page_1_data[i] != chip_ds_msg.page_1_data[i])
		{
			printf("\nPage 1 data write operation error!\n");
			return -8;
		}
	}	
	printf("\nPage 1 data verfify OK!\n");
	//set page_num	
	chip_ds_msg.page_num = 0;
	printf("\nUse %d page data for MAC computation.\n", chip_ds_msg.page_num);
	//set no anonymous mode
	chip_ds_msg.anon = 0;		
	
	//Read MAC from chip, this step does not need to send secret to chip. ONLY 32 byte challenge data is used.
	ret = ioctl(fd, SHA256_COMPUTE_READ_MAC, &chip_ds_msg);
	if(ret < 0)
	{
		printf("DS IOCTL COMPUTE_READ_MAC FAIL!\n");
		return -9;
	}
	
	anon = chip_ds_msg.anon;
	
	page_num = chip_ds_msg.page_num;
	if(page_num == 0)
		memcpy(page_data, chip_ds_msg.page_0_data, 32);
	else
		memcpy(page_data, chip_ds_msg.page_1_data, 32);
	memcpy(challenge, chip_ds_msg.challenge, 32);
	memcpy(secret, chip_ds_msg.secret, 32);
	memcpy(man_id, chip_ds_msg.man_id, 2);
	memcpy(chip_mac, chip_ds_msg.mac, 32);
	printf("chip Mac:\n");
	for (i = 0; i < 32; i++)
	{	
		printf(" 0x%02x ", chip_mac[i]);		
	}
	
	if(!Compute_Software_Mac(page_num, page_data, challenge, rom_id, secret, man_id, anon, chip_mac))
	{
		printf("MAC compare ERROR!\n");
		return -10;
	}
	else
		printf("MAC compare OK!\n");	

	close(fd);
	return 0;		
}



