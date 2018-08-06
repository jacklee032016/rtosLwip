#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################
RTOS_SUB_DIRS += \
	portable/MemMang/ \
	portable/GCC/POSIX \



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

RTOS_SRC += \
	portable/GCC/POSIX/port.c \
	portable/MemMang/heap_4.c \
	list.c \
	queue.c \
	tasks.c \
	timers.c \
	event_groups.c \

# never use it
#	$(RTOS_HOME)/croutine.c \



SRC += \
	$(RTOS_SRC) \


TARGET_LIB:=libRtos

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post
