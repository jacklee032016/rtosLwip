# Build

** toolchain

download toolchain `arm-gnu-toolchain-6.3.1.508-linux.any.x86_64.tar.gz` from 
`https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads`

support zephyr arm gcc toolchains also;


** build

* Modify Rule.mak to define AN767 board (SAME70Q20) or SAM E70 XPlaind board (SAME70Q21);

* 'cd supports; make': 
   * build 3 libraries: rtos, lwip, rtk;

* 'cd an767; make':
   * build libAsf, libBsp;
   * build bootloader and rtos binaries;

