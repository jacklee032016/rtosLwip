#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################
RTOS_SUB_DIRS += \
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
	sam/drivers/matrix/ \
	sam/drivers/pmc/ \
	sam/drivers/pwm/ \
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



CODE_SUBDIRS = \
	$(RTOS_SUB_DIRS) \
	

# 
CFLAGS += \
	$(RTOS_FLAGS) \
	$(RTOS_HEADER) \
	$(CPACK_FLAGS) \
	-Iinclude \
	-Ithirdparty/CMSIS/Include \
	-Icommon/boards/ \
	-Icommon/services/clock/ \
	-Icommon/services/clock/samv71/ \
	-Icommon/services/ioport/ \
	-Icommon/services/ioport/sam/ \
	-Icommon/services/serial/ \
	-Icommon/services/serial/sam_uart/ \
	-Icommon/utils/ \
	-Icommon/utils/interrupt/ \
	-Icommon/utils/stdio/ \
	-Icommon/utils/stdio/stdio_serial/ \
	-Isam/ \
	-Isam/boards/ \
	-Isam/boards/samv71_xplained_ultra/ \
	-Isam/drivers/ \
	-Isam/drivers/pmc/ \
	-Isam/drivers/uart/ \
	-Isam/drivers/usart/ \
	-Isam/drivers/xdmac/ \
	-Isam/utils/ \
	-Isam/utils/cmsis/ \
	-Isam/utils/cmsis/samv71/ \
	-Isam/utils/cmsis/samv71/include/ \
	-Isam/utils/cmsis/samv71/include/component/ \
	-Isam/utils/cmsis/samv71/include/instance/ \
	-Isam/utils/cmsis/samv71/include/pio/ \
	-Isam/utils/cmsis/samv71/source/ \
	-Isam/utils/cmsis/samv71/source/templates/ \
	-Isam/utils/cmsis/samv71/source/templates/gcc/ \
	-Isam/utils/fpu/ \
	-Isam/utils/header_files/ \
	-Isam/utils/linker_scripts/ \
	-Isam/utils/linker_scripts/samv71/ \
	-Isam/utils/linker_scripts/samv71/samv71q21/ \
	-Isam/utils/linker_scripts/samv71/samv71q21/gcc/ \
	-Isam/utils/make/ \
	-Isam/utils/preprocessor/ \
	-Isam/utils/syscalls/ \
	-Isam/utils/syscalls/gcc/ \
	-Ithirdparty/ \
	-Ithirdparty/CMSIS/ \
	-Ithirdparty/CMSIS/Include/ \
	-Ithirdparty/CMSIS/Lib/ \
	-Ithirdparty/CMSIS/Lib/GCC/ \
	-I. \
	-I../os3/include/ \
	



###################################################################
# define source files
###################################################################

BSP_SRC += \
	sam/drivers/matrix/matrix.c \
	sam/drivers/pwm/pwm.c \
	sam/drivers/pmc/pmc.c \
	sam/drivers/pmc/sleep.c \
	sam/drivers/uart/uart.c \
	sam/drivers/usart/usart.c \
	sam/drivers/xdmac/xdmac.c \
	common/services/clock/samv71/sysclk.c \
	common/services/serial/usart_serial.c \
	common/utils/interrupt/interrupt_sam_nvic.c \
	common/utils/stdio/read.c \
	common/utils/stdio/write.c \
	sam/boards/samv71_xplained_ultra/init.c \
	sam/utils/cmsis/samv71/source/templates/gcc/startup_samv71.c \
	system_init.c \

#	sam/utils/syscalls/gcc/syscalls.c \


SRC += \
	$(BSP_SRC) \


TARGET_LIB:=libUcosBsp

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(RULE_DIR)/Makefile.post
