#
#  $
#


include $(RULE_DIR)/Rules.mak

NETWORK_SUPPORT=YES

BOOTLOAD_HOME:=$(RULE_DIR)

###################################################################
# define directory for object files
###################################################################

CODE_SUBDIRS += \
	board \
	chip \
	utils \
	


CFLAGS += \
	-I$(BOOTLOAD_HOME)/include \
	-I$(BOOTLOAD_HOME)/include/cmsis \
	-I$(BOOTLOAD_HOME)/include/board \
	-I$(BOOTLOAD_HOME)/include/chip \
	-I$(BOOTLOAD_HOME)/include/chip/samv7 \
	-I$(BOOTLOAD_HOME)/include/chip/samv7/instance \
	-I$(BOOTLOAD_HOME)/include/chip/samv7/component \
	-DDONT_USE_CMSIS_INIT 
	
  

ASM_FLAGS= 


###################################################################
# define source files
###################################################################
BOARD_SRC += \
	board/lowlevel.c	\
	board/trace.c	\
	board/dbg_console.c	\


CHIP_SRC += \
	chip/supc.c \
	chip/pmc.c \
	chip/pio.c \
	chip/uart.c \
	
NOT_USED += \
	chip/wdt.c \
	chip/efc.c \
	chip/flashd.c \


UTILS_SRC += \
	utils/stdio.c \
	utils/string.c \
	utils/retarget.c \


SRC += \
	$(BOARD_SRC) \
	$(CHIP_SRC) \
	$(UTILS_SRC) \
	bootloader.c \
	cstartup.c \



#TARGET_LIB:=bootloader

#TARGET_LIB_SHARED:=$(TARGET_LIB).so
#TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
EXE=bootloader
LOCAL_ALL:=$(EXE).bin

include $(ROOT_DIR)/Makefile.post

#LIBS:=-Wl,-L$(BIN_DIR)/lib -lASF -lThirdparty 

#LIBS= -Wl,-Lthirdparty/CMSIS/Lib/GCC -larm_cortexM7lfsp_math_softfp -lm  

# -Wl,-L../thirdparty/CMSIS/Lib/GCC -larm_cortexM7lfsp_math_softfp: float is not used now
# -Wl,--cref: create references  -g $(OPTIMIZATION) 
LDFLAGS = -nostartfiles -mcpu=cortex-m4 -mthumb -Wl,-Map=$(OUTPUT).map,--cref,--gc-sections

$(LOCAL_ALL):$(OBJS)
		$(CC) -nostartfiles  -nostdlib -mthumb -Wl,-Map=$(EXE).map,--cref,--gc-sections -mthumb -mcpu=cortex-m7 -T boot.lds $(OBJS) -o $(EXE).elf 
		$(OBJCOPY) -O binary $(EXE).elf $@
		$(MOVE) $@ $(BIN_DIR)/sbin
		$(MOVE) $(EXE).elf $(BIN_DIR)/sbin
		$(MOVE) $(EXE).map $(BIN_DIR)/sbin

