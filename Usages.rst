=========================
Usage of Python utilites
=========================

Programming AN767
======================

Programming in factory:

* Programming bootloader and OS for MCU with ICE programmer;
* Programming FPGA;
* Enter into OS, command line input "tx 1|0" to set board as TX or RX;
* Press button more than 3 seconds, make device into default factory configuration, different for TX and RX;


Button

* Press button when power is on, stay in bootloader;
* Pressing and releasing button less and equal 3 seconds, then reboot;
* Pressing and releasing button more than 3 seconds, then reset to factory configuration;

::

	lfx 64: write the backup FPGA firmware into 0x0040,0000, eg. 4M


TFTP update firmware:

	tftp -m binary $IP $PORT(69) -c put $FIRMWARE_FILE firmOs|firmFpga


HTTP update firmware:
		/mcuUpdate
    /fpgaUpdate


run './run.py'

::
 ./run.py --help
 ./run.py COMMAND --help, 

for example, `./run.py httpOs --help
     

Usage
--------

1. Search all nodes in LAN:
::

 ./run search -d

2. Lookup one node info:
::

 run find $IP


3. Update firmware 
 * by HTTP
::

	run httpOs --help
	run httpOs 192.168.168.130
	
	run httpFpga --help
	run httpFpga 192.168.168.130
	run httpFpga --file /media/sf_rtos/0826/top_Angelica_N_767_tx.bin 192.168.168.64
	

 * by TFTP
::

  run tftpOs 192.168.168.130
  run tftpFpga 192.168.168.130



Send JSON IP command to node
::
 ./run.py setParams '{"ip":"192.168.168.121","isDhcp":1}' 192.168.168.120 -d

  