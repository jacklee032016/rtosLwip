

By default a factory new SAM E70 chip will boot SAM-BA boot loader located in the ROM, 
not the flashed image. This is determined by the value of GPNVM1 (General-Purpose NVM 
bit 1). 
The flash procedure will ensure that GPNVM1 is set to 1 changing the default behavior 
to boot from Flash.


If your chip has a security bit GPNVM0 set you will be unable to program flash memory 
or connect to it via a debug interface. The only way to clear GPNVM0 is to perform a 
chip erase procedure that will erase all GPNVM bits and the full contents of the SAM E70 
flash memory:

   * With the board power off, set a jumper on the J200 header.
   * Turn the board power on. The jumper can be removed soon after the power is on (flash 
     erasing procedure is started when the erase line is asserted for at least 230ms)
