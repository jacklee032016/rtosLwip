#
#  $
#


include $(RULE_DIR)/Rules.mak

NETWORK_SUPPORT=YES


###################################################################
# define directory for object files
###################################################################

CODE_SUBDIRS += \
	utils \
	net \
	cmds	\
	tasks \
	hw \
	 \

BSP_HEADER += \
	-Ibsp \



CFLAGS += \
	$(RTOS_FLAGS) \
	$(LWIP_FLAGS) \
	$(BSP_HEADER) \
	$(OS_HEADER) \
	$(ASF_HEADER) \
	$(RTOS_HEADER) \
	$(LWIP_HEADER) \
	$(RTK_HEADER) \
	-I.		\
	
  

ASM_FLAGS= 


###################################################################
# define source files
###################################################################


NETWORK_SRC += \
	net/eosNetSysArch.c \
	net/eosNetMac.c \
	net/eosNetPhy.c \
	net/eosNetInit.c \


CMDS_SRC += \
	cmds/eosCliCmds.c \
	cmds/eosCliNetworks.c \


TASKS_SRC += \
	tasks/eosTaskConsole.c \
	tasks/eosTimerJobs.c \

HW_SRC += \
	hw/eosHwRtk.c \
	hw/eosHwFpgaDrv.c \
	hw/eosHwTC.c \

	

UTILS_SRC += \
	utils/eosSyscalls.c \


SRC += \
	$(NETWORK_SRC) \
	$(HW_SRC) \
	$(CMDS_SRC) \
	$(UTILS_SRC) \
	$(TASKS_SRC) \
	eosCStartup.c	\


#	eosMain.c	\


TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL=

EXE_TX=$(EXE).tx
EXE_RX=$(EXE).rx

EXES=$(EXE_TX).bin $(EXE_RX).bin 

include $(RULE_DIR)/Makefile.post



#LIBS= -Wl,-Lthirdparty/CMSIS/Lib/GCC -larm_cortexM7lfsp_math_softfp -lm  
LIBS= -Wl,-L$(BIN_DIR) -lBsp -lAsf $(SUPPORT_LIBS)

# -Wl,-L../thirdparty/CMSIS/Lib/GCC -larm_cortexM7lfsp_math_softfp: float is not used now --specs nosys.specs 
# -Wl,--cref: create references -Wl,-Map=$(EXE).map,--cref,--gc-sections 
#$(LOCAL_ALL):$(OBJS)
#		$(CC) -mthumb --specs=nano.specs --specs nosys.specs -mcpu=cortex-m7  -lm -Wl,--entry=Reset_Handler -T $(LINK_SCRIPT) $(OBJS) $(LIBS) -o $(EXE).elf 
#		$(OBJCOPY) -O binary $(EXE).elf $@
#		$(MOVE) $(EXE).elf $(BIN_DIR)/sbin
#		$(MOVE) $@ $(BIN_DIR)/sbin

# -nostartfiles : when __libc_init_array is not used


$(OBJ_DIR)/eosMainRx.o: eosMain.c	
	$(CC) $(C_FLAGS) $(CFLAGS) -DTX_VERSION=0 -c $< -o $(OUTPUT_FLAG) $@ 


$(OBJ_DIR)/eosMainTx.o: eosMain.c	
	$(CC) $(C_FLAGS) $(CFLAGS) -DTX_VERSION=1 -c $< -o $(OUTPUT_FLAG) $@ 


$(EXE_TX).bin:$(OBJS) $(OBJ_DIR)/eosMainTx.o
		$(CC) -mthumb --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m7 -Wl,-Map=$(EXE_TX).map -Wl,--entry=Reset_Handler -T $(LINK_SCRIPT) $^ $(LIBS) -lm -o $(EXE_TX).elf 
		$(OBJCOPY) -O binary $(EXE_TX).elf $@
		$(COPY) $(EXE_TX).elf $(BIN_DIR)/$(EXE_TX).$(BUILDTIME).elf
		$(MOVE) $(EXE_TX).elf $(BIN_DIR)
		$(COPY) $@ $(BIN_DIR)/$(EXE_TX).$(BUILDTIME).bin
		$(MOVE) $@ $(BIN_DIR)
		$(MOVE) $(EXE_TX).map $(BIN_DIR)

$(EXE_RX).bin:$(OBJS) $(OBJ_DIR)/eosMainRx.o
		$(CC) -mthumb --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m7 -Wl,-Map=$(EXE_RX).map -Wl,--entry=Reset_Handler -T $(LINK_SCRIPT) $^ $(LIBS) -lm -o $(EXE_RX).elf 
		$(OBJCOPY) -O binary $(EXE_RX).elf $@
		$(COPY) $(EXE_RX).elf $(BIN_DIR)/$(EXE_RX).$(BUILDTIME).elf
		$(MOVE) $(EXE_RX).elf $(BIN_DIR)
		$(COPY) $@ $(BIN_DIR)/$(EXE_RX).$(BUILDTIME).bin
		$(MOVE) $@ $(BIN_DIR)
		$(MOVE) $(EXE_RX).map $(BIN_DIR)


all:$(EXES)
