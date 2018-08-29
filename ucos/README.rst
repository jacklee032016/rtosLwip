
uC OS III V3.05.01 Atmel SAM E70Q2x
######################################
08.28, 2016


Need some work porting to E70Q2X, especially on interrupt vector table;


Port to Atmel Cortex-M7
=========================
# **cpu**
  * cpu.h: definitions of CPU;
  * bsp (cpu_bsp.c) is not used;
  * cpu_c.c: Interrupt priority/Enable/Disable, from Cortex-M4;
  * cpu_a.s: basic instructions of CPU;

# **ucLib**
  * not used them all;
  * lib_mem_a.s, from Cortex-M4;

# **ucos3**
  * port directory, generic for Cortex-M:
  * os_cpu_c.c: SysTick, Stack, Task hook; 
  * os_cpu_a.s:
	

Build for Atmel
======================
From ATSAMV71Q21(CPU)/SAMV71-XULTRA(board)

**Build in Studio**
::

	arm-none-eabi-gcc -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -mthumb -Wl,-Map="uart-demo.map" -Wl,--start-group -larm_cortexM7lfsp_math_softfp -lm -los3 -lbsp  -Wl,--end-group -L"../cmsis/linkerScripts" -L"../src/ASF/thirdparty/CMSIS/Lib/GCC" -L"../../libbsp/Debug" -L"../../libos3/Debug"  -Wl,--gc-sections -mcpu=cortex-m7 -Wl,--entry=Reset_Handler -Wl,--cref -mthumb -T../src/ASF/sam/utils/linker_scripts/samv71/samv71q21/gcc/flash.ld  

**BIN format**
::

	arm-none-eabi-objcopy -O binary uart-demo.elf uart-demo.bin

**HEX format**
::

	arm-none-eabi-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature  uart-demo.elf uart-demo.hex

**EEP format**
::

	arm-none-eabi-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary uart-demo.elf uart-demo.eep || exit 0

**List Symbols**
::

	arm-none-eabi-objdump -h -S uart-demo.elf  > uart-demo.lss
	
**SREC format**
::

	arm-none-eabi-objcopy -O srec -R .eeprom -R .fuse -R .lock -R .signature  uart-demo.elf uart-demo.srec

**size**
::

	arm-none-eabi-size uart-demo.elf
	
