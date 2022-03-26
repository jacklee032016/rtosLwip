#!/bin/awk -f

# for debugging the layout of binary and show memory usage; 
# Feb.7th, 2018 J.L.

# arm-none-eabi-nm -a BIN/images.AN767/rtosLwipAN767.bin.elf | grep _etext


function getPosition(tag)
{
    if (tag == "")
    {
    		print "No tag is defined"
        return 0
    }

		NM_CMD="arm-none-eabi-nm -a "
		ELF_FILE="BIN/images.AN767/rtosLwipAN767.bin.elf"
		ELF_FILE="BIN/images.E7oXpld/rtosLwipE7oXpld.bin.elf"
    command = NM_CMD ELF_FILE" | grep '" tag "'"
    
#    print command
    command|getline result
    close(command)
    split(result, post, " ")

		post[1]=strtonum( "0x" post[1] )
#		print post[1]		
    return post[1]
}

BEGIN {

#    print "begin"
    
#    command = "arm-none-eabi-nm -a Rtos.BIN.AN767/sbin/rtosLwipAN767.elf | grep 't .text'"
#    command|getline result
#    close(command)
#    split(result, textBegin, " ")

		textBegin=0
		textEnd=0

#    print "Code(Flash)Section:" textEnd[1] "-" textBegin[1] "=" textEnd[1]-textBegin[1] " bytes"
    
	textBegin=getPosition("t .text")
	textEnd=getPosition("_etext")

	bssBegin=getPosition("B _sbss")
	bssEnd=getPosition("B _ebss")

	dataBegin=getPosition("_srelocate")
	dataEnd=getPosition("_erelocate")

	stackBegin=getPosition("_sstack")
	stackEnd=getPosition("_estack")

	heapBegin=getPosition("_sheap")
	heapEnd=getPosition("_eheap")
	
	ramEnd=getPosition("_ram_end_")

  print "Binary summary:"
  
  printf("Flash: 0x%x -- 0x%x;\tLength:%d KB (%d KB)\n",textBegin, textEnd, (textEnd-textBegin+1023)/1024, 0x100000/1024 )
  printf("\tCode Section :\tStart: 0x%x;\tLength: %d bytes\n",textBegin, textEnd-textBegin)

  printf("\nRAM:\t0x%x -- 0x%x;\tLength: %d KB\n", dataBegin, ramEnd, (ramEnd-dataBegin+1023)/1024);
  printf("\tData Section:\tStart:0x%x;\tLength: %d bytes\n", dataBegin, dataEnd-dataBegin);
  printf("\tBSS Section:\tStart:0x%x;\tLength: %d bytes\n", bssBegin, bssEnd-bssBegin);
    
  printf("\tStack Section:\tStart:0x%x;\tLength: %d bytes\n", stackBegin, stackEnd-stackBegin);
  printf("\tHeap Section:\tStart:0x%x;\tLength: %d bytes\n", heapBegin, heapEnd-heapBegin);

  printf("\nFree RAM:0x%x--0x%x;\tLength: %d bytes (%d KB)\n", heapEnd, ramEnd, ramEnd-heapEnd, (ramEnd-heapEnd+1023)/1024);
  
}

