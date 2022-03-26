    Program 

## Program AN767 board

* connect SAM-ICE to JTAG (P2) onboard;
* connect USB-serial cable to P5(p10[TXD], p9[GND], p8[RXD]);

### Program in Microchop Studio
   * tools\"Device Programming"\:
       * tools: SAM-ICE
       * device: ATSAME70Q20
       * interface: SWD
   * apply
   * device Info
   * Memory

** Program bootloader
   * program bootloader.elf, default to start address of flash;
   
   * program OS: 
       * program with bootloader;
       * program with Microchop Studio: 
           * program rtosLwipAN767.bin.bin to address of 0x00420000
           * bootloader load os at 0x00420000;

           

** an767 system
   * bootloader is at 0x400000;
   * OS is at 0x420000;
   
** SAM E70 plaind board
   * bootloader at 0x400000;
   * OS at 0x400000;
   
* details defined in link script files;
   

## GPNVM bits

* After erase chip, GPNVM is 0x000, now CPU boot from ROM, not from flash;
* GPNVM must be changed as 0x042 or 0x002, CPU boot from flash;

   
   

