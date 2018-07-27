
				Command Line Readme
											 Feburary 15, 2018	Jack Lee


FPGA registers read/write command:
	'register [address] [value]':
		- optional parameters 'address' and 'value';
		- no parameter: read all registers;
		- only 'address': read the register at 'address';
		- both 'address' and 'value': write value into register in that address, then read and check the result in register;
		
		Notes:
			- 'address' and 'value' are in format of hexadecimal, such as '0x0a' or '0a';
			- 'address' and 'value' are size of 8 bits;

Command Line Access Interfaces:
	Console on serial port;
	telnet in IP network;

System Configuration:
	Static IP Address: 192.168.168.120/255.255.255.0/192.168.168.1(Gateway);
	MAC Address: 00:04:25:1c:A0:04;
	
	Serial Port: 
		baud rate: 115200;
		data bit: 8; no parity; stop bit:1;

