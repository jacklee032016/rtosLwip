#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################
RTOS_SUB_DIRS += \
		.



CODE_SUBDIRS = \
	$(RTOS_SUB_DIRS) \
	

# 
CFLAGS += \
	$(RTOS_FLAGS) \
	$(RTOS_HEADER) \
	$(LWIP_HEADER) \
	$(CPACK_FLAGS) \
	$(APP_HEADER) \



###################################################################
# define source files
###################################################################


SRC += \
	sys_arch.c \


TARGET_LIB:=libLwipSys

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post
