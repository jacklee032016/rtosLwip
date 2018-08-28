#
#  $
#


include $(RULE_DIR)/Rules.mak

NETWORK_SUPPORT=YES


###################################################################
# define directory for object files
###################################################################

CODE_SUBDIRS += \
	common/boards/ \
	common/services/clock/samv71/ \
	common/services/ioport/ \
	common/services/ioport/sam/ \
	common/services/serial/ \
	common/services/serial/sam_uart/ \
	common/utils/ \
	common/utils/interrupt/ \
	common/utils/stdio/ \
	common/utils/stdio/stdio_serial/ \
	sam/ \
	sam/boards/ \
	sam/boards/samv71_xplained_ultra/ \
	sam/drivers/ \
	sam/drivers/pmc/ \
	sam/drivers/uart/ \
	sam/drivers/usart/ \
	sam/drivers/xdmac/ \
	sam/utils/ \
	sam/utils/cmsis/ \
	sam/utils/cmsis/samv71/ \
	sam/utils/cmsis/samv71/include/ \
	sam/utils/cmsis/samv71/include/component/ \
	sam/utils/cmsis/samv71/include/instance/ \
	sam/utils/cmsis/samv71/include/pio/ \
	sam/utils/cmsis/samv71/source/ \
	sam/utils/cmsis/samv71/source/templates/ \
	sam/utils/cmsis/samv71/source/templates/gcc/ \
	sam/utils/fpu/ \
	sam/utils/header_files/ \
	sam/utils/linker_scripts/ \
	sam/utils/linker_scripts/samv71/ \
	sam/utils/linker_scripts/samv71/samv71q21/ \
	sam/utils/linker_scripts/samv71/samv71q21/gcc/ \
	sam/utils/make/ \
	sam/utils/preprocessor/ \
	sam/utils/syscalls/ \
	sam/utils/syscalls/gcc/ \
	thirdparty/ \
	thirdparty/CMSIS/ \
	thirdparty/CMSIS/Include/ \
	thirdparty/CMSIS/Lib/ \
	thirdparty/CMSIS/Lib/GCC/ \
	config/	\


BSP_HEADER += \
	-Ibsp \


BSP_HOME=../../libs/bsp

CFLAGS += \
	$(RTOS_FLAGS) \
	$(RTOS_HEADER) \
	$(CPACK_FLAGS) \
	-I. \
	-I$(BSP_HOME) \
	-I$(BSP_HOME)/thirdparty/CMSIS/Include \
	-I$(BSP_HOME)/common/boards/ \
	-I$(BSP_HOME)/common/services/clock/ \
	-I$(BSP_HOME)/common/services/clock/samv71/ \
	-I$(BSP_HOME)/common/services/ioport/ \
	-I$(BSP_HOME)/common/services/ioport/sam/ \
	-I$(BSP_HOME)/common/services/serial/ \
	-I$(BSP_HOME)/common/services/serial/sam_uart/ \
	-I$(BSP_HOME)/common/utils/ \
	-I$(BSP_HOME)/common/utils/interrupt/ \
	-I$(BSP_HOME)/common/utils/stdio/ \
	-I$(BSP_HOME)/common/utils/stdio/stdio_serial/ \
	-I$(BSP_HOME)/sam/ \
	-I$(BSP_HOME)/sam/boards/ \
	-I$(BSP_HOME)/sam/boards/samv71_xplained_ultra/ \
	-I$(BSP_HOME)/sam/drivers/ \
	-I$(BSP_HOME)/sam/drivers/pmc/ \
	-I$(BSP_HOME)/sam/drivers/uart/ \
	-I$(BSP_HOME)/sam/drivers/usart/ \
	-I$(BSP_HOME)/sam/drivers/xdmac/ \
	-I$(BSP_HOME)/sam/utils/ \
	-I$(BSP_HOME)/sam/utils/cmsis/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/include/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/include/component/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/include/instance/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/include/pio/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/source/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/source/templates/ \
	-I$(BSP_HOME)/sam/utils/cmsis/samv71/source/templates/gcc/ \
	-I$(BSP_HOME)/sam/utils/fpu/ \
	-I$(BSP_HOME)/sam/utils/header_files/ \
	-I$(BSP_HOME)/sam/utils/linker_scripts/ \
	-I$(BSP_HOME)/sam/utils/linker_scripts/samv71/ \
	-I$(BSP_HOME)/sam/utils/linker_scripts/samv71/samv71q21/ \
	-I$(BSP_HOME)/sam/utils/linker_scripts/samv71/samv71q21/gcc/ \
	-I$(BSP_HOME)/sam/utils/make/ \
	-I$(BSP_HOME)/sam/utils/preprocessor/ \
	-I$(BSP_HOME)/sam/utils/syscalls/ \
	-I$(BSP_HOME)/sam/utils/syscalls/gcc/ \
	-I$(BSP_HOME)/thirdparty/ \
	-I$(BSP_HOME)/thirdparty/CMSIS/ \
	-I$(BSP_HOME)/thirdparty/CMSIS/Include/ \
	-I$(BSP_HOME)/thirdparty/CMSIS/Lib/ \
	-I$(BSP_HOME)/thirdparty/CMSIS/Lib/GCC/ \
	-Iconfig \
	-I. \
	-I../../libs/os3/include/ \
	
	
  

ASM_FLAGS= 


###################################################################
# define source files
###################################################################


SRC += \
	main.c \
	syscalls.c \
	
	
BSP_SRC += \
	sam/drivers/matrix/matrix.c \
	sam/drivers/pwm/pwm.c \
	led.c \
	system_init.c \
	common/services/clock/samv71/sysclk.c \
	common/utils/interrupt/interrupt_sam_nvic.c \
	sam/boards/samv71_xplained_ultra/init.c \
	sam/drivers/pmc/pmc.c \
	sam/drivers/pmc/sleep.c \
	sam/utils/cmsis/samv71/source/templates/gcc/startup_samv71.c \
	sam/utils/syscalls/gcc/syscalls.c


EXE=ucos

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL=$(EXE).bin 

include $(RULE_DIR)/Makefile.post



#LIBS= -Wl,-Lthirdparty/CMSIS/Lib/GCC -larm_cortexM7lfsp_math_softfp -lm  
LIBS= -Wl,-L$(BIN_DIR) -lUcosBsp -lrtos 

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
