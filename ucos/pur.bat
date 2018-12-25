echo Program by USB debug cable(EDBG).....
rem echo 

atprogram -t EDBG -i SWD -d ATSAME70Q21 program -f BIN/images.E7oXpld/ucos.elf --erase --verify
