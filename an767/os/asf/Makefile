#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################

CODE_SUBDIRS = \
	drivers \
	

# dependent on CMSIS 
CFLAGS += \
	$(LWIP_FLAGS) \
	$(BSP_HEADER) \
	$(OS_HEADER) \
	$(ASF_HEADER) \
	-I. \
	$(LWIP_HEADER) \


###################################################################
# define source files
###################################################################

DRIVERS_SRC += \
	drivers/mpu.c \
	drivers/sleep.c \
	drivers/tc.c \
	drivers/matrix.c \
	drivers/rstc.c \
	drivers/gmac_phy.c \
	
	
#	drivers/efc.c \
#	drivers/flash_efc.c \


SRC += \
	$(DRIVERS_SRC) \
	interrupt_nvic.c \


TARGET_LIB:=libAsf

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(RULE_DIR)/Makefile.post
