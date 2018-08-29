#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################
CODE_SUBDIRS = \
	drivers/ \
	utils/	\
	

# 
CFLAGS += \
	$(BSP_HEADER) \
	$(CPACK_FLAGS) \
	-Iinclude \
	-I. \
	$(UCOS_HEADER) \
	

###################################################################
# define source files:
###################################################################

BSP_SRC += \
	drivers/pwm.c \
	drivers/pmc.c \
	drivers/sleep.c \
	drivers/uart.c \
	drivers/usart.c \
	drivers/xdmac.c \
	drivers/sysclk.c \
	drivers/interrupt_sam_nvic.c \
	utils/usart_serial.c \
	utils/read.c \
	utils/write.c \
	init.c \
	system_init.c \
	startup_samv71.c \

#	sam/utils/syscalls/gcc/syscalls.c \
#	drivers/matrix.c \


SRC += \
	$(BSP_SRC) \


TARGET_LIB:=libUcosBsp

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(RULE_DIR)/Makefile.post
