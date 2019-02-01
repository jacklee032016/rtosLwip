REM  modify this variable as your environment
set BOOTLOAD_FILE=BIN/images.AN767/bootloaderAN767.elf

echo Programming Bootloader.....
echo Booloader file "%BOOTLOAD_FILE%"

atprogram -t samice -i SWD -d ATSAME70Q20 -v program -f %BOOTLOAD_FILE%
REM  atprogram -t samice -i SWD -d ATSAME70Q20 -v program -f bootloaderAN767.elf


