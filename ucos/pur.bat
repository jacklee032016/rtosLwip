echo Program by USB debug cable(EDBG).....
echo 

atprogram -t EDBG -i SWD -d ATSAME70Q21 -v program -f BIN/images.E7oXpld/ucos.elf
