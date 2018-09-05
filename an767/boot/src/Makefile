#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################

CODE_SUBDIRS += \
	utils \
	drivers\

#	modem \
	


CFLAGS += \
	-I$(BOOT_HOME)/include \
	$(BSP_HEADER) \
	-I. \
	$(LWIP_HEADER) \
	$(CPACK_FLAGS) \

  

ASM_FLAGS= 


###################################################################
# define source files
###################################################################
CMDS_SRC += \
	bootCmds.c \

#	bootCmdFlash.c \
	
	
MODEM_SRC += \
	modem/xyModem.c \
	modem/crc16.c \


UTILS_SRC += \
	utils/bootUtils.c \
	utils/retarget.c \

#	utils/string.c \
#	utils/stdio.c \

SRC += \
	$(CMDS_SRC)	\
	$(UTILS_SRC)	\
	bootMain.c \
	cstartup.c \

#	$(MODEM_SRC) \


#TARGET_LIB:=bootloader

#TARGET_LIB_SHARED:=$(TARGET_LIB).so
#TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
EXE=bootloader$(BOARD_NAME)
LOCAL_ALL:=$(EXE).bin

include $(RULE_DIR)/Makefile.post


CMN_LIB:=-Wl,-L$(SUPPORT_HOME)/BIN/images.$(BOARD_NAME) -lLwip

#

$(LOCAL_ALL):$(OBJS)
#		$(CC) -nostartfiles -nostdlib -mthumb -Wl,-Map=$(EXE).map,--cref,--gc-sections -mthumb -mcpu=cortex-m7 -T flash.lds $(OBJS) -lgcc -Wl,-L$(BIN_DIR) -lbsp $(CMN_LIB) -o $(EXE).elf 
		$(CC) --specs=nano.specs -specs=nosys.specs -mthumb -Wl,-Map=$(EXE).map,--cref,--gc-sections -mthumb -mcpu=cortex-m7 -T flash.lds $(OBJS) -Wl,-L$(BIN_DIR) -lbsp $(CMN_LIB) -o $(EXE).elf 
		$(OBJCOPY) -O binary $(EXE).elf $@
		$(COPY) $(EXE).elf $(BIN_DIR)/$(EXE).$(BUILDTIME).elf
		$(MOVE) $(EXE).elf $(BIN_DIR)
		$(COPY) $@ $(BIN_DIR)/$(EXE).$(BUILDTIME).bin
		$(MOVE) $@ $(BIN_DIR)
		$(MOVE) $(EXE).map $(BIN_DIR)


#		$(CC) -mthumb --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m7  -lm -Wl,--entry=Reset_Handler -T flash.lds $(OBJS) -o $(EXE).elf 


