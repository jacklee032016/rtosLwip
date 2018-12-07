echo Program by USB debug cable(EDBG).....
rem echo 

atprogram -t EDBG -i SWD -d ATSAME70Q21 read -f flash.bin -s 10240 --offset 0x400000 --format bin
