REM Modify GPNVM, 10.17, 2018
REM 0x0000, 0x0200, 0x4200, etc
REM 0xLLHH : LL is first 8-bits in NVM, HH only 1 bit (bit8) in NVM

atprogram -t samice -i swd -d atsame70q20 write -fs --values 0x4200 -o 0 -v

