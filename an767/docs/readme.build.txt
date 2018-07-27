
				README for Basis of RTOS Project
											 January, 2018	Jack Lee

Runtime:
		1. BSP
				Only dependent itself and ASF;
		2. Boot
				Dependent on BSP;
		3. ASF:
				Dependent on BSP and asf;
		4. OS:
				Dependent on all modules;
				
				Notes:
						syscalls.c in only needed by OS, because it is needed by nano libc;


Support libraries:
		1. RTK library:
				build independently, without related with any other library;
				When debugging, it calls some macros defined by compact.h;

		2. Free RTOS:
				Only dependent on FreeRtosConfig.h, which dependent on clock frequency definition 'SystemCoreClock';
				Hardcode 'SystemCoreClock' in FreeRtosConfig.h, so simplify build of free rtos;
				define	__NVIC_PRIO_BITS from compact; hardcode it as 3 for Cortext-M7, so no compact.h is needed;
		
				Port.c of FreeRtos provide 3 ISRs(PendingSV, sysTick, SVC, which are used in the vector table);

		3. LWIP:
				sys_arch dependent on RTOS: sys_arch.h refer to FreeRTOS.h;


Feb.23rd
Link options:
	bootloader:
			No libc or nano libc is needed;
			syscalls.c is not needed;
			Call float point division when calculate the frequency needed for delay; This instruction is implemented in gcc (-lgcc);

	OS:
			dependent on nano libc;
			nano libc also dependent on syscalls.c (which is from gcc)
			Calls __libc_init_array to initialize libc; 
					Only when __libc_init_array is not used, '-nostartfiles ' can be used;
					
			'-Wl,--gc-sections' must be used; otherwise
					 arm-none-eabi/lib/thumb/v7e-m/crt0.o: In function `_start':
					 			(.text+0x64): undefined reference to `__bss_start__' 
					 			(.text+0x68): undefined reference to `__bss_end__'		



