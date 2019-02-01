REM  modify this variable as your environment
set TX_ELF_FILE=BIN/images.AN767/rtosLwipAN767.tx.elf

echo  Programming TX....
echo  RX Programming file is "%TX_ELF_FILE%"
atprogram -t samice -i SWD -d ATSAME70Q20 -v program -f %TX_ELF_FILE%

