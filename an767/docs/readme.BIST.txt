
				README for Built-In Self Test
											 April, 2018	Jack Lee


Internal Flash Test (EFC Embedded Flash Controller) Flash
	EraseSize: 8192 bytes
	Page Size, Read/Write Size: 512 bytes
	
	ifr r/w/e page:
			Read/Write/Erase sector based on page No:
			Write/Erase only for page 2032~2047: zone for backup configuration
							

SPI Flash, through FPGA
	Erase Size, Sector: 64KB
	Page Size, read/write size: 256B
		BIST:
				read Flash ID
				erase/write/read one sector, at the sector no.64, which is the start address of second FPAG image;

		sfr [startPage [count]]: read content at page of start page
				start page: 0~131070
				count: 1~131070
				
