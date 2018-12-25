
Works Log of uC OS III V3.05.01 Atmel SAM E70Q2x
##########################################################
12.08, 2018

12.24, 2018
------------------
Add Lwip supports and test it;
3 tasks: MAC, tcpip and console;
USB functions implemented in ISR;
Dummp IRQ handler: segment fault in C programs;


12.13, 2018
------------------
**Debug OSTimeDly() blocked in task main when console task is used**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
* Task console has same priority as main(startup) task  
  * low priority of task console(10-->12), then OSTimeDly works for both tasks;
  * task tick also has priority of 10;
* Task console waits on UART port by blocking:
  * task console does not receive string from UART really, everything is fine;
  * task console receives string by blocking and waiting on UART;
  
* Make task console is driven by IRQ;
  * Don't printf() in ISR of serial port;
  * OSFlagPend() must add CONSUME flag, otherwise, OSFlagPend() can by used again and again;


12.12, 2018
------------------
* Add console task and command line interface:
   * When 2 tasks running, time tick is not working??
   * boot command can't run from RTOS/bootloader??
   * when OS starts, it display "_W F _S"?? : from USB device driver;

12.11, 2018
-----------------
* Test Program with 3 tasks and IPC;
* USB program with XDMAC controller to serial port and OSFlag support;
  * validate uC/OS kernel and start task to initialize hardware;
  * hardware configuration in start task;
  * ISR in uC/OS;
  * Test XDMAC controller;
  * Every application has its configuration of OS;
  

12.09, 2018
-----------------
* Add USB device modules: vendor/msc/cdc;
* Build every module independently;
* Add common module of header file;

12.08, 2018
-----------------
* copy sleepmgr.* from common/services of ASF;
* Add code for USB device driver, USB device control and USB device interface;
* batch file to program flash with command: add erase option to make it work;
* port to SAME70Q21 Xplait board;
