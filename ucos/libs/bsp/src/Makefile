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
	drivers/usb/udc \
	drivers/usb/uhc \
	

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

USB_SRC += \
	drivers/usb/usbhs_device.c \
	drivers/usb/udc/udc.c \

BSP_SRC += \
	drivers/pwm.c \
	drivers/pmc.c \
	drivers/sleep.c \
	drivers/sleepmgr.c \
	drivers/uart.c \
	drivers/usart.c \
	drivers/xdmac.c \
	drivers/sysclk.c \
	drivers/interrupt_sam_nvic.c \
	drivers/pio.c \
	drivers/pio_handler.c \
	utils/usart_serial.c \
	utils/read.c \
	utils/write.c \
	init.c \
	system_init.c \
	startup_samv71.c \
	ui.c \
	
	

#	sam/utils/syscalls/gcc/syscalls.c \
#	drivers/matrix.c \


SRC += \
	$(USB_SRC) \
	$(BSP_SRC) \


TARGET_LIB:=libUcosBsp

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(RULE_DIR)/Makefile.post
