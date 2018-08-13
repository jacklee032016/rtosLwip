# rule for all modules

############## Common for all modules
# support PC environments
# ARCH=
ARCH=arm


CPU_E70Q20=YES
#CPU_E70Q20=


# released or debug version, different on debug and log info£¬2007.03.15
# must be 'release' or 'debug'
EDITION=debug
#EDITION=release

ifeq ($(EDITION),release)
	C_FLAGS += -D__EXT_RELEASE__
else	
endif 

ifeq ($(ARCH),arm)
#	C_FLAGS += -D__ARM_CMN__=1 -DARCH_ARM=1  -DARCH_X86=0 -DARCH_X86_32=0 
	CROSS_COMPILER=arm-none-eabi-
	LDFLAGS+=  
	flag=
	C_FLAGS +=-DARM -DARCH_ARM=1 
	
else
	ARCH=X86
#	C_FLAGS +=-D$(ARCH) -DARCH_X86=1 -DARCH_X86_32=1 -DARCH_ARM=0 
	EXTENSION=
endif


ifeq ($(CPU_E70Q20),YES)
	BOARD_NAME=AN767
	CFLAGS += -D__SAME70Q20__ -DEXTLAB_BOARD=1 -DEXT_LAB
	LINK_SCRIPT =$(OS_HOME)/src/linkers/q20flash.ld
else
	BOARD_NAME=E7oXpld
	CFLAGS += -D__SAME70Q21__ -DEXTLAB_BOARD=0 -DEXT_LAB
	LINK_SCRIPT =$(OS_HOME)/src/linkers/q21flash.ld
endif

BIN_DIR=$(ROOT_DIR)/BIN/images.$(BOARD_NAME)
#OBJ_DIR=$(ROOT_DIR)/BIN/objs.$(BOARD_NAME)
OBJ_DIR=objs


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



############## definitions for different modules

AN767_HOME:=$(RULE_DIR)/an767


BSP_HOME:=$(AN767_HOME)/bsp
BOOT_HOME:=$(AN767_HOME)/boot
OS_HOME:=$(AN767_HOME)/os

ASP_HOME:=$(OS_HOME)/asf

MAIN_HOME=src

BSP_HEAD_HOME=$(BSP_HOME)/include


MAIN_HEAD_HOME:=$(ROOT_DIR)/$(MAIN_HOME)




# macro '__SAME70Q21__' and '__SAME70Q20__' for Q20 and Q21 CPU;
# macro 'MUX_LAB' for all modification made by MuxLab, both on Q20 and Q21;
# macro 'MUXLAB_BOARD' for all modification only for AN767 board;


CFLAGS += 

EXE=rtosLwip$(BOARD_NAME).bin


# support libries
SUPPORT_HOME:=$(RULE_DIR)/supports

RTK_HOME=$(SUPPORT_HOME)/rtk
RTOS_HOME=$(SUPPORT_HOME)/rtos
LWIP_HOME=$(SUPPORT_HOME)/lwip

SUPPORT_LIBS:=-Wl,-L$(SUPPORT_HOME)/BIN/images.$(BOARD_NAME) -lRtos -lLwip

# RTK only dependent on chip and compiler
ifeq ($(CPU_E70Q20),YES)
	RTK_HEADER += \
		-I$(RTK_HOME) \
		-I$(RTK_HOME)/asicdrv \
		-I$(RTK_HOME)/asicdrv/basic 

	RTK_FLAGS+= -DRTL8307H_DEBUG

	SUPPORT_LIBS+=	-lRtk
endif




# gmac, tc and pmc only used in network
BSP_HEADER= \
	-I$(BSP_HEAD_HOME) \
	-I$(BSP_HEAD_HOME)/clock \
	-I$(BSP_HEAD_HOME)/cmsis \
	-I$(BSP_HEAD_HOME)/drivers \
	-I$(BSP_HEAD_HOME)/same70 \


OS_HEADER= \
	-I$(OS_HOME)/include \

ASF_HEADER += \
	-I$(ASP_HOME)/include \
 
 

RTOS_HEADER= \
	-I$(RTOS_HOME)/include \
  -I$(RTOS_HOME)/portable/GCC/ARM_CM7/r0p1 \

LWIP_HEADER= \
	-I$(LWIP_HOME)/lwip/src/include \
	-I$(LWIP_HOME)/lwip/src/include/ipv4 \
	-I$(LWIP_HOME)/ports/sam/ \
	$(BSP_HEADER) \




MUX_HEADER= \
	-I$(MAIN_HEAD_HOME)/include \


CFLAGS += -DROOT_DIR='"$(ROOT_DIR)"' -I$(ROOT_DIR) 


###################################################################
# define directories for header file and build flags
###################################################################


RTOS_FLAGS+=

LWIP_FLAGS+=-DLWIP_DEBUG -DLWIP_V2=1 


CFLAGS += -mthumb -mcpu=cortex-m7 -mfloat-abi=softfp -mfpu=fpv5-sp-d16 -DARM_MATH_CM7=true \
	-O1 -fdata-sections -ffunction-sections -mlong-calls -g3 -Wall \
	-pipe -fno-strict-aliasing -Wall -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith \
	-std=gnu99 -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs \
	-Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return \
	-Wmissing-declarations -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wredundant-decls \
	-Wunreachable-code -Wcast-align --param max-inline-insns-single=500 


# can't use this options, pbuf in Lwip and gmac of atmel must be alligned differently, such as 8 bytes border or others
# CPACK_FLAGS =	-fpack-struct
# -fpack-struct, add 05.07,2018,JL

# -Wlong-long: disable ISO C90 not support 'long long' warns in RTK SDK
# -Wpacked : disable warns for 'packed' in LwIP protocols
# -Wnested-externs : nested extern declaration

# for FreeRTOS, declarations of sysclk_get_cpu_hz() 
#CFLAGS += -Werror=implicit-function-declaration	

