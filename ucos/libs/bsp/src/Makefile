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
	usb/udc \
	usb/uhc \
	usb/cdc \
	usb/msc \
	usb/vendor \
	
	

# 
CFLAGS += \
	$(RTOS_FLAGS) \
	$(RTOS_HEADER) \
	$(CPACK_FLAGS) \
	$(BSP_HEADER) \
	-Iinclude \
	-I. \
	$(UCOS_HEADER) \
	

###################################################################
# define source files:
###################################################################


USB_DEVICE_SRC += \
	usb/usbhs_device.c \
	usb/udc/udc.c \

DEVICE_CDC_SRC += \
	usb/cdc/usbCdcDesc.c \
	usb/cdc/usbCdc.c \
	usb/cdc/usbCdcMain.c \
	usb/cdc/usbCdcUart.c \


DEVICE_MSC_SRC += \
	usb/msc/usbMscDesc.c \
	usb/msc/usbMsc.c \
	usb/msc/usbMscMain.c \

DEVICE_VENDOR_SRC += \
	usb/vendor/usbVendorDesc.c \
	usb/vendor/usbVendor.c \
	usb/vendor/usbVendorMain.c \


USB_SRC += \
	$(USB_DEVICE_SRC) \
	$(DEVICE_VENDOR_SRC) \
	
#	$(DEVICE_CDC_SRC) \
#	$(DEVICE_VENDOR_SRC) \
	

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
	syscalls.c \
	system_init.c \
	startup.c \
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
