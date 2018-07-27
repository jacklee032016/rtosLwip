
				Test of AT SAME70Q21
											 January, 2018	Jack Lee

Jan.20th
	Reconstruct the build environment for all components:
		Add Rule.mak, remove Makefile.pre;
		ASF, Thirdparty and other sources are built independently;
	
Jan.17~19
	Add I2C support; 
	Access PCA9445 chip with I2C;
	Access RTL8355 chip and make it link with I2C services;

Jan.16th, Tuesday
	add UART0 support and port to SAM E70Q20, MuxLab board;
	add source for RTL8305H_SDK_V3_5_2011_8_10;

Jan.15th, Monday
	Add netconn for LwIP application:
		netconn_accept: invalid acceptmbox: memory is not enough, only suitable for one application;
		
Jan,12th, Friday
	Implement FreeRTOS CLI plus with normal commands;
	

Jan.11th, Thursday
	Add 'apps' for different applications based on RTOP and SAM E70 board;
		3 items must be defined for every app:
			Directories of object files;
			Header file flags for preprocess;
			Source files to build;
			

	LwIP + Ethernet KSZ8081RNA:
		thirdparty/lwIP/		: IP protocol stack;
		sam/drivers/gmac/		: GMAC driver;
		sam/drivers/tc/ 		: timer used by LwIP;
		sam/components/ethernet_phy/kzs8081rna/: ksz8081 PHY;
		network/						: initial ethernet and timer for protocol stack;
		network/httpd/			: HTTP server
		
	Note:
		sam/components: directory for external components onboard, eg. component only onboard, not in chipset;
		

	Running FreeRTOS 10.0.0 on SAM E70Q21 Evaluation Board, port from newly release of FreeRTOS;
		
		Minimal requirement for LED and USART: 115200 bps, 8 data bits, 1 stop bit, no parity, no flow control;
			1. common
			common/utils/stdio/read.o
			common/utils/stdio/write.o
			common/services/serial/usart_serial.o
			common/services/clock/same70/sysclk.o
			common/utils/interrupt/interrupt_sam_nvic.o
			
			2. sam
			sam/boards/same70_xplained/init.o
			sam/utils/cmsis/same70/source/templates/gcc/startup_same70.o
			sam/utils/cmsis/same70/source/templates/system_same70.o
			sam/utils/syscalls/gcc/syscalls.o
			sam/drivers/mpu/mpu.o
			sam/drivers/pio/pio.o
			sam/drivers/pio/pio_handler.o
			sam/drivers/pmc/pmc.o
			sam/drivers/pmc/sleep.o
			sam/drivers/uart/uart.o
			sam/drivers/usart/usart.o
			sam/drivers/tc/tc.o
			sam/drivers/matrix/matrix.o
			
			3. 
			thirdparty/freertos/portable/gcc/ARM_CM7/r0p1/port.o
			thirdparty/freertos/portable/memmang/heap_4.o
			thirdparty/freertos/list.o
			thirdparty/freertos/queue.o
			thirdparty/freertos/tasks.o
			thirdparty/freertos/timers.o
			thirdparty/freertos/croutine.o
			thirdparty/freertos/event_groups.o
			
			main.o
			

Jan.10th,Wedneday, 2018

rtosLwIP:
		ASP+RTOS+LwIP(kzs8081)+HTTP Server;
		ASF:
			sam/components/ethernet_phy/: kzs8081
			sam/drivers/gmac/: MAC control;
			sam/drivers/tc/: timer counter;
			
			Thirdpartiy/:
				freeRtos:7.3.0
				lwip:1.4.1
			config:	
				NO ICache and DCache
				config_th.h
				lwiptops.h


rtos:
		based on ASF:
			common, thirdparty, sam are from ASP;
		FreeRTOS task + LED + USART(115200);
		Print 'Iteration number is: xxx';

uartHello
		'Hello world!' in USART(USB) port, baudrate 38400;
		Baudrate : config/hpl_usart_config.h
		
		Debug the problem of 'sam.h' is not found in hal/utils/include/parts.h:
			Add include directory in IDE:
				CFLAGS += -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\arm\cmsis\5.0.1\CMSIS\Include" 
				CFLAGS += -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\SAME70_DFP\2.2.91\same70a\include" 
			The project file contains old directory for the development environments;	


Flash size: 2M,   start: 0x0040,0000, length: 0x0020,0000
SRAM  size: 384K, start: 0x2040,0000, length: 0x0006,0000

simulator       No serialnumber found
edbg            ATML2637010000000000

