#
#  $
#


include $(RULE_DIR)/Rules.mak

NETWORK_SUPPORT=YES


###################################################################
# define directory for object files
###################################################################

CODE_SUBDIRS += \
		.


CFLAGS += \
	$(BSP_HEADER) \
	$(CPACK_FLAGS) \
	$(UCOS_HEADER) \
	-I. \
  

ASM_FLAGS= 


###################################################################
# define source files
###################################################################


SRC += \
	main.c \
	syscalls.c \
	
	

EXE=ucos

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL=$(EXE).bin 

include $(RULE_DIR)/Makefile.post



#LIBS= -Wl,-Lthirdparty/CMSIS/Lib/GCC -larm_cortexM7lfsp_math_softfp -lm  
LIBS= -Wl,-L$(BIN_DIR) -lUcosBsp -lRtos 

# -Wl,-L../thirdparty/CMSIS/Lib/GCC -larm_cortexM7lfsp_math_softfp: float is not used now --specs nosys.specs 
# -Wl,--cref: create references -Wl,-Map=$(EXE).map,--cref,--gc-sections 
#$(LOCAL_ALL):$(OBJS)
#		$(CC) -mthumb --specs=nano.specs --specs nosys.specs -mcpu=cortex-m7  -lm -Wl,--entry=Reset_Handler -T $(LINK_SCRIPT) $(OBJS) $(LIBS) -o $(EXE).elf 
#		$(OBJCOPY) -O binary $(EXE).elf $@
#		$(MOVE) $(EXE).elf $(BIN_DIR)/sbin
#		$(MOVE) $@ $(BIN_DIR)/sbin

# -nostartfiles : when __libc_init_array is not used
$(LOCAL_ALL):$(OBJS)
		$(CC) -mthumb --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m7 -Wl,-Map=$(EXE).map -Wl,--entry=Reset_Handler -T $(LINK_SCRIPT) $(OBJS) $(LIBS) -lm -o $(EXE).elf 
		$(OBJCOPY) -O binary $(EXE).elf $@
		$(MOVE) $(EXE).elf $(BIN_DIR)
		$(MOVE) $@ $(BIN_DIR)
		$(MOVE) $(EXE).map $(BIN_DIR)
