# rule for all modules

# support PC environments
# ARCH=
ARCH=arm

LWIP_VERSOIN=2

ASF_HOME:=asf
THIRD_HOME:=thirdparty
CONFIG_HOME:=config

RTOS_HOME=freertos
RTK_HOME=rtk

ifeq ($(ARCH),arm)
	LWIP_HOME=lwip-1.4.1
else
	LWIP_HOME=lwip-2.0.3
endif

MAIN_HOME=src

ASF_HEAD_HOME=$(ROOT_DIR)/$(ASF_HOME)
THIRD_HEAD_HOME:=$(ROOT_DIR)/$(THIRD_HOME)
CONFIG_HEAD_HOME:=$(ROOT_DIR)/$(CONFIG_HOME)

RTOS_HEAD_HOME:=$(THIRD_HEAD_HOME)/$(RTOS_HOME)
LWIP_HEAD_HOME:=$(THIRD_HEAD_HOME)/$(LWIP_HOME)
RTK_HEAD_HOME:=$(THIRD_HEAD_HOME)/$(RTK_HOME)

MAIN_HEAD_HOME:=$(ROOT_DIR)/$(MAIN_HOME)


# released or debug version, different on debug and log info£¬2007.03.15
# must be 'release' or 'debug'
EDITION=debug
# release

ifeq ($(ARCH),arm)
#	C_FLAGS += -D__ARM_CMN__=1 -DARCH_ARM=1  -DARCH_X86=0 -DARCH_X86_32=0 
	CROSS_COMPILER=arm-none-eabi-
	LDFLAGS+=  
	flag=
	C_FLAGS +=-DARM
	
else
	ARCH=X86
#	C_FLAGS +=-D$(ARCH) -DARCH_X86=1 -DARCH_X86_32=1 -DARCH_ARM=0 
	EXTENSION=
endif


ifeq ($(CPU_E70Q20),YES)
	BOARD_NAME=AN767
	CFLAGS += -D__SAME70Q20__ -DMUXLAB_BOARD=1 -DRTL8307H_DEBUG
	LINK_SCRIPT =$(ASF_HEAD_HOME)/sam/utils/linker_scripts/same70/same70q20/gcc/flash.ld
else
	BOARD_NAME=E7oXpld
	CFLAGS += -D__SAME70Q21__ -DMUXLAB_BOARD=0 
	LINK_SCRIPT =$(ASF_HEAD_HOME)/sam/utils/linker_scripts/same70/same70q21/gcc/flash.ld
endif

# macro '__SAME70Q21__' and '__SAME70Q20__' for Q20 and Q21 CPU;
# macro 'MUX_LAB' for all modification made by MuxLab, both on Q20 and Q21;
# macro 'MUXLAB_BOARD' for all modification only for AN767 board;
CFLAGS += -DMUX_LAB

EXE=rtosLwip$(BOARD_NAME)



BIN_DIR=$(ROOT_DIR)/Rtos.BIN.$(BOARD_NAME)
OBJ_DIR=$(ROOT_DIR)/Rtos.OBJ.$(BOARD_NAME)


ifeq ($(ARCH),X86)
else
	ARCH=arm
endif


CC	= $(CROSS_COMPILER)gcc
CXX 	= $(CROSS_COMPILER)g++ 
STRIP	= $(CROSS_COMPILER)strip
LD	= $(CROSS_COMPILER)ld
RANLIB 	= $(CROSS_COMPILER)ranlib
STRIP 	= $(CROSS_COMPILER)strip
AR 	= $(CROSS_COMPILER)ar
OBJCOPY 	= $(CROSS_COMPILER)objcopy

ASM = yasm

RM	= rm -r -f
MKDIR	= mkdir -p
MODE	= 700
OWNER	= root
CHOWN	= chown
CHMOD	= chmod
COPY	= cp
MOVE	= mv

LN		= ln -sf




# configuration options for manage this project
#BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
BUILDTIME := $(shell TZ=CN date -u "+%Y_%m_%d")
GCC_VERSION := $(shell $(CC) -dumpversion )

RELEASES_NAME=$(NAME)_$(GCC_VERSION)_$(ARCH)_$(EDITION)_$(BUILDTIME).tar.gz  


export BUILDTIME
export RELEASES_NAME



# gmac, tc and pmc only used in network
SAME_HEADER= \
	-I$(ASF_HEAD_HOME)/sam/utils \
	-I$(ASF_HEAD_HOME)/sam/utils/header_files \
	-I$(ASF_HEAD_HOME)/sam/utils/preprocessor \
	-I$(ASF_HEAD_HOME)/sam/utils/fpu \
	-I$(ASF_HEAD_HOME)/sam/utils/cmsis/same70/include \
	-I$(ASF_HEAD_HOME)/sam/utils/cmsis/same70/source/templates \
	-I$(ASF_HEAD_HOME)/sam/boards \
	-I$(ASF_HEAD_HOME)/sam/boards/same70_xplained \
	-I$(ASF_HEAD_HOME)/sam/drivers/pio \
	-I$(ASF_HEAD_HOME)/sam/drivers/mpu \
	-I$(ASF_HEAD_HOME)/sam/drivers/uart \
	-I$(ASF_HEAD_HOME)/sam/drivers/usart \
	-I$(ASF_HEAD_HOME)/sam/drivers/matrix \
	-I$(ASF_HEAD_HOME)/sam/drivers/twihs \
	-I$(ASF_HEAD_HOME)/sam/drivers/gmac \
	-I$(ASF_HEAD_HOME)/sam/drivers/tc \
	-I$(ASF_HEAD_HOME)/sam/drivers/pmc \


COMMON_HEADER= \
	-I$(ASF_HEAD_HOME)/common/boards \
	-I$(ASF_HEAD_HOME)/common/services/clock \
	-I$(ASF_HEAD_HOME)/common/utils	\
	-I$(ASF_HEAD_HOME)/common/services/ioport \
	-I$(ASF_HEAD_HOME)/common/services/gpio \
	-I$(ASF_HEAD_HOME)/common/services/serial/sam_uart \
	-I$(ASF_HEAD_HOME)/common/services/serial \
	-I$(ASF_HEAD_HOME)/common/services/delay \
	-I$(ASF_HEAD_HOME)/common/services/delay/sam \
	-I$(ASF_HEAD_HOME)/common/utils/stdio/stdio_serial \

ASF_HEADER += \
	$(SAME_HEADER) \
	$(COMMON_HEADER) \
 
 
# CMSIS from thirdparty only works as binary library
CMSIS_HEADER= \
	-I$(THIRD_HEAD_HOME)/CMSIS/Include \
	

RTOS_10_HEADER= \
	-I$(RTOS_HEAD_HOME)/include \
  -I$(RTOS_HEAD_HOME)/portable/GCC/ARM_CM7/r0p1 \
	-I$(RTOS_HEAD_HOME)/common/include \

LWIP_HEADER= \
	-I$(LWIP_HEAD_HOME)/src/include \
	-I$(LWIP_HEAD_HOME)/src/include/ipv4 \
	-I$(LWIP_HEAD_HOME)/ports/sam/include \


ifeq ($(CPU_E70Q20),YES)
	BSP_HEADER += \
		-I$(RTK_HEAD_HOME) \
		-I$(RTK_HEAD_HOME)/asicdrv \
		-I$(RTK_HEAD_HOME)/asicdrv/basic \
		
endif

THIRDPARTY_HEADER= \
	$(CMSIS_HEADER) \
	$(RTOS_10_HEADER) \
	$(LWIP_HEADER) \
	$(BSP_HEADER) \


MUX_HEADER= \
	-I$(MAIN_HEAD_HOME)/include \


CFLAGS += -DROOT_DIR='"$(ROOT_DIR)"' -I$(ROOT_DIR) 

SHARED_CFLAGS += -I$(SHARED_HOME)/include
SHARED_LDFLAGS += -L$(SHARED_HOME)/Linux.bin.$(ARCH)/lib 


###################################################################
# define directories for header file and build flags
###################################################################

#CFLAGS += -Wall -Wextra $(SOURCE_HEADER) $(PORTABLE_HEADER) $(COMMON_HEADER) $(DEMO_HEADER) \
#		$(RASP_FLAGS) -fpic -ffreestanding

# -Dprintf=iprintf

CFLAGS += -mthumb -DDEBUG -DLWIP_DEBUG -DBOARD=SAME70_XPLAINED -Dscanf=iscanf -DARM_MATH_CM7=true  -D__FREERTOS__  \
	-O1 -fdata-sections -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m7  -mfloat-abi=softfp -mfpu=fpv5-sp-d16 \
	-pipe -fno-strict-aliasing -Wall -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith \
	-std=gnu99 -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs \
	-Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return \
	-Wmissing-declarations -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wredundant-decls \
	-Wunreachable-code -Wcast-align --param max-inline-insns-single=500 

# -Wlong-long: disable ISO C90 not support 'long long' warns in RTK SDK
# -Wpacked : disable warns for 'packed' in LwIP protocols
# -Wnested-externs : nested extern declaration

# for FreeRTOS, declarations of sysclk_get_cpu_hz() 
#CFLAGS += -Werror=implicit-function-declaration	

