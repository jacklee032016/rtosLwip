
echo Programming Bootloader by EDBG(USB Debugging Cable) for E70Xpld board.....

atprogram -t EDBG -i SWD -d ATSAME70Q21 -v program -f BIN/images.E7oXpld/bootloaderE7oXpld.elf

