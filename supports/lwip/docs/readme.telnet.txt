
				Readme for Telnet 
											 July 18,2018	Jack Lee

Add new options: only support one client
		Delay for about 4 seconds for the second connection, because PU TTY will quit after connection is broken.
		This will make user wonder what happens behind the exit of PU TTY.
		
		So add poll function for this new pcb_cbr to make the broken of connection will delay 4 seconds;
		

Automatic timeout for about 30*2 seconds:
		After 30*2 seconds without ant activity, output message and break telnet connection;
		poll function execute per 2 seconds;

Check string ending with 0x0D and 0x0A:
		All telnet input 0x0D and 0x0A at end of every line, even with just RETURN input in client;

PU TYY client:
		Can receive any reply after the first prompt;
		
		Resolution:
			PU TTY always send packet as small as possible, most time it send packet in 2 or 1 bytes.
			These packets with 1 or 2 byte length is ignored by telnet server;
			
			So add code to handle and recreate the whole command from these shortest packets correctly;
			