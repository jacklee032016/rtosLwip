REM  modify this variable as your environment
set RX_ELF_FILE=BIN/images.AN767/rtosLwipAN767.rx.elf

echo  Programming RX....
echo  RX Programming file is "%RX_ELF_FILE%"
atprogram -t samice -i SWD -d ATSAME70Q20 -v program -f %RX_ELF_FILE%

