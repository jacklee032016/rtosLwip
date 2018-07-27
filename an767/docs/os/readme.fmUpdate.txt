
				Readme for Firmware Update
											 July 19, 2018	Jack Lee


Design:
		1. Update first image of FPGA, and the second image always keep primary usable version;
		2. Network(tftp and HTTP) updating only write to the backup zone of SPI flash; 
			After reboot, bootload read images(both RTOS/FPGA) from backup zone, and rewrite to the correct position of firmware;
		3. Old bootloader must compatible with new RTOS:
				Bootloader can not be updated; 
				When new version of RTOS release, bootloader must be capable update RTOS/FPGA firmwares;			
		4. System only enter and stay in bootloader if button is pressed when bootup;


Command Line:
		FPGA firmware update:
				Default, update to the first image;
				When add parameter of sector#64, update to the second image;
		
		RTOS firmware update:
				Only used in bootloader;
				Write to MCU flash directly;
				

