
				LwIP for FreeRTOS and SAM E70 Q20/21
											 January, 2018	Jack Lee

Jan.,31st, 2018
	Update to 2.0.3
	Nearly to the limitation of RAM size:  region `ram' overflowed by 82264 bytes:
		decrease the buffer sizes;

header include and usage:
	opt.h 
		--> lwipopts.h --> conf_net.h --> gmac.h --> ASF/SAM headers, another options for compiler ;
		--> debug.h
					--> arch.h: define u32_t and others;
							--> ports/**/cc.h: build and macros for platform dependent;

	

Network:
	muxNetPhy.c:		MDC/MDIO and I2C to access management info from PHY;
	muxNetMac.c:		Network interface in LWIP, for MAC layer of GMAC in SAM E70;
	muxNetStacks.c:	Start protocol stack of LwIP;


Jan,15th, Friday
	Port from 1.4.1:
		src/core/init.c  --> lwip_init.c
			add 'lwip_sanity_check()' and some checks for macros;
		src/core/timer.c  --> lwip_timers_141.c
			Some mini modifications;

		remove src/core/

